<?hh // strict

namespace ElasticExport\Helper;


use Plenty\Modules\Item\DataLayer\Models\Record;

/**
 * Class ItemDescriptionHelper
 * @package ElasticExport\Helper
 */
class ItemExportHelper
{
    /**
     * Get name.
     *
     * @param  Record $item
     * @param  array  $settings
     * @return string
     */
    public function getName(Record $item, array<string, string>$settings):string
	{
		if(array_key_exists('nameId', $settings))
		{
			switch($settings['nameId'])
			{
				case 3:
					return $item->itemDescription->name3;

				case 2:
					return $item->itemDescription->name2;

				case 1:
				default:
					return $item->itemDescription->name1;
			}
        }

        return $item->itemDescription->name1;
    }

    /**
     * Get description.
     *
     * @param  Record        $item
     * @param  array<string, string>$settings
     * @param  int           $defaultDescriptionLength
     * @return string
     */
    public function getDescription(Record $item, array<string, string>$settings, int $defaultDescriptionLength):string
    {
        if(array_key_exists('descriptionLength', $settings))
        {
            $descriptionLength = $settings['descriptionLength'];
        }
        else
        {
            $descriptionLength = $defaultDescriptionLength;
        }

        if(array_key_exists('descriptionType', $settings))
        {
            switch($settings['descriptionType'])
            {
                case 'shortDescription':
                    return substr(strip_tags($item->itemDescription->shortDescription), 0, $descriptionLength);

                case 'technicalData':
                    return substr(strip_tags($item->itemDescription->technicalData), 0, $descriptionLength);

                case 'description':
                default:
                    return substr(strip_tags($item->itemDescription->description), 0, $descriptionLength);
            }
        }

        return substr(strip_tags($item->itemDescription->description), 0, $descriptionLength);
    }
}
