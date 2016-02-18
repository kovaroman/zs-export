<?hh // strict
namespace ElasticExport\Filters;

use Plenty\Modules\DataExchange\Contracts\Filters;
use Plenty\Modules\DataExchange\Models\FormatSetting;
use Plenty\Modules\Helper\Services\ArrayHelper;

class Belboon extends Filters
{
    /*
	 * @var ArrayHelper
	 */
	private ArrayHelper $arrayHelper;

    /**
     * Belboon constructor.
     * @param ArrayHelper $arrayHelper
     */
    public function __construct(ArrayHelper $arrayHelper)
    {
		    $this->arrayHelper = $arrayHelper;
    }

    /**
     * Generate filters.
     * @param  array<FormatSetting> $formatSettings = []
     * @return array
     */
    public function generateFilters(array<FormatSetting> $formatSettings = []):array<string, mixed>
    {
        $settings = $this->arrayHelper->buildMapFromObjectList($formatSettings, 'key', 'value');

        return [];;
    }
}
