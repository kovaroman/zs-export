<?hh // strict
namespace ElasticExport\Filters;

use Plenty\Modules\DataExchange\Contracts\Filters;
use Plenty\Modules\DataExchange\Models\FormatSetting;

/**
 * Class IdealoDE
 * @package ElasticExport\Filters
 */
class IdealoDE extends Filters
{

    public function generateFilters(array<FormatSetting> $formatSettings = []):array<string, mixed>
    {
		$searchFilter = [
			'variationBase.isActive?' => [],
		];

		return $searchFilter;
    }
}
