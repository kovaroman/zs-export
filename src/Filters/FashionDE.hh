<?hh // strict
namespace ElasticExport\Filters;

use Plenty\Modules\DataExchange\Contracts\Filters;
use Plenty\Modules\DataExchange\Models\FormatSetting;

/**
 * Class FashionDE
 * @package ElasticExport\Filters
 */
class FashionDE extends Filters
{

    public function generateFilters(array<FormatSetting> $formatSettings = []):array<string, mixed>
    {
        //code here
        return [];
    }
}
